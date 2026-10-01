const int WINDOW = 5;              // samples per average
const int NOISE_AMPLITUDE = 20;

int buffer[WINDOW];                // circular buffer (starts at 0)
long sum = 0;                      // running sum
int idx = 0;                       // current slot in the buffer
int count = 0;                     // samples collected so far

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  randomSeed(analogRead(A1));      // A1 floating, used only as seed
}

void loop() {
  if (count < WINDOW) count++;     // buffer still filling up

  int clean = analogRead(A0);
  int noisy = constrain(clean + random(-NOISE_AMPLITUDE, NOISE_AMPLITUDE + 1), 0, 1023);

  sum -= buffer[idx];              // remove the oldest value
  buffer[idx] = noisy;             // store the newest
  sum += noisy;

  float filtered = (float)sum / count;

  Serial.print(clean);
  Serial.print(",");
  Serial.print(noisy);
  Serial.print(",");
  Serial.println(filtered, 1);

  idx = (idx + 1) % WINDOW;        // wrap around
  delay(25);
}