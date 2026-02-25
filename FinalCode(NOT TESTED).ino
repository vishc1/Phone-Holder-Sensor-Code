/*
  AD8317 MIN-ONLY 3-state classifier
  States:
    - NO_DIPS
    - PARTIAL_DIPS
    - FULL_DIPS

  You said your logger outputs VoutMin only, so:
  - Each "point" = 1 second window
  - We only compute VoutMin in that window (no averages used for detection)

  If using external AREF = 3.3V, wire 3.3V -> AREF and GND -> GND
*/

const int PIN_VOUT = A0;

// If you use AREF external 3.3V:
const bool USE_EXTERNAL_AREF = true;
const float AREF_V = 3.3;

// Timing
const unsigned long BASELINE_MS = 10000;  // first 10 seconds baseline
const unsigned long WINDOW_MS   = 1000;   // each printed point is 1 second
const unsigned long SAMPLE_MS   = 2;      // sampling inside the window (500 Hz-ish)

// Thresholds (start here; tune if needed)
const float PARTIAL_DIP_V = 0.12;  // baseline - Vmin >= 0.12 => PARTIAL
const float FULL_DIP_V    = 0.25;  // baseline - Vmin >= 0.25 => FULL

// Debounce (consecutive windows required to change state)
const int CONFIRM_WINDOWS = 3;

// (Optional) Power conversion placeholders (not used for detection)
const float intercept = 2.0;
const float slope = -0.022;

enum State { NO_DIPS, PARTIAL_DIPS, FULL_DIPS };
State state = NO_DIPS;
State pending = NO_DIPS;
int pendingCount = 0;

unsigned long tStart;
unsigned long winStart;
unsigned long lastSample;

float winMinV = 999.0;

// Baseline stats for VoutMin (Welford mean/std)
long baseN = 0;
double baseMean = 0.0;
double baseM2 = 0.0;

float readVoutVolts() {
  int raw = analogRead(PIN_VOUT);
  float ref = USE_EXTERNAL_AREF ? AREF_V : 5.0;
  return raw * (ref / 1023.0);
}

void welfordUpdate(double x) {
  baseN++;
  double d = x - baseMean;
  baseMean += d / baseN;
  double d2 = x - baseMean;
  baseM2 += d * d2;
}

double baseStd() {
  if (baseN < 2) return 0.0;
  return sqrt(baseM2 / (baseN - 1));
}

const char* stateName(State s) {
  if (s == NO_DIPS) return "NO_DIPS";
  if (s == PARTIAL_DIPS) return "PARTIAL_DIPS";
  return "FULL_DIPS";
}

float voltsToDbm(float v) {
  return (v - intercept) / slope;
}

void setup() {
  if (USE_EXTERNAL_AREF) {
    analogReference(EXTERNAL);   // Uno: use AREF pin
    delay(1000);
  }

  Serial.begin(9600);
  delay(200);

  tStart = millis();
  winStart = millis();
  lastSample = 0;

  Serial.println("Baseline 10s: keep slot EMPTY (NO PHONE). Using VoutMin only.");
}

void loop() {
  unsigned long now = millis();

  // Sample inside the window to find VoutMin
  if (now - lastSample >= SAMPLE_MS) {
    lastSample = now;
    float v = readVoutVolts();
    if (v < winMinV) winMinV = v;
  }

  // End of 1-second window => one "point"
  if (now - winStart >= WINDOW_MS) {
    float vMin = (winMinV < 900.0) ? winMinV : readVoutVolts();

    // Baseline period: first 10 seconds (10 windows)
    if (now - tStart < BASELINE_MS) {
      welfordUpdate(vMin);

      Serial.print("[BASELINE] VoutMin: ");
      Serial.print(vMin, 3);
      Serial.print(" V | Power(min): ");
      Serial.print(voltsToDbm(vMin), 2);
      Serial.println(" dBm");

      // Print baseline stats right at the end (approx)
      if (BASELINE_MS - (now - tStart) < WINDOW_MS) {
        Serial.println("---- BASELINE ENDING ----");
      }
    } else {
      float bMean = (float)baseMean;  // baseline mean of VoutMin (no dips)
      float dip = bMean - vMin;       // bigger dip => lower Vmin

      // Classify THIS window
      State windowClass = NO_DIPS;
      if (dip >= FULL_DIP_V) windowClass = FULL_DIPS;
      else if (dip >= PARTIAL_DIP_V) windowClass = PARTIAL_DIPS;
      else windowClass = NO_DIPS;

      // Debounce state changes
      if (windowClass == state) {
        pending = state;
        pendingCount = 0;
      } else {
        if (windowClass == pending) pendingCount++;
        else { pending = windowClass; pendingCount = 1; }

        if (pendingCount >= CONFIRM_WINDOWS) {
          state = pending;
          pendingCount = 0;
          Serial.print(">>> STATE = ");
          Serial.println(stateName(state));
        }
      }

      // Print like your current logger style (min-only)
      Serial.print("VoutMin: ");
      Serial.print(vMin, 3);
      Serial.print(" V | dipFromBaselineMinMean: ");
      Serial.print(dip, 3);
      Serial.print(" V | Power(min): ");
      Serial.print(voltsToDbm(vMin), 2);
      Serial.print(" dBm | windowClass: ");
      Serial.print(stateName(windowClass));
      Serial.print(" | state: ");
      Serial.println(stateName(state));
    }

    // Reset window
    winStart = now;
    winMinV = 999.0;
  }
}
