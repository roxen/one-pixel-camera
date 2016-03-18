/**
 * Adjust imageWidth and imageHeight to the "sensor size". This
 * defines the area that is scanned.
 * Adjust x resolution to desired size. y resolution is calculated
 * proportionally.
 */
const int imageWidth = 40; // mm
const int imageHeight = 30; // mm
const int resolutionX = 320; // pixels

const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;

const int X = 0;
const int Y = 1;

int StepsX = 0;
int StepsY = 0;
boolean DirectionX = true;
boolean DirectionY = true;

const int resolutionY = resolutionX * imageHeight / imageWidth;

// 10000 steps == 92mm
const int stepsPerMillimeter = 10000 / 92;

int stepsXPerStep = imageWidth * stepsPerMillimeter / resolutionX;
int stepsYPerStep = imageHeight * stepsPerMillimeter / resolutionY;

byte line[resolutionX];

void setup()
{
  Serial.begin(57600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(IN1 + 4, OUTPUT);
  pinMode(IN2 + 4, OUTPUT);
  pinMode(IN3 + 4, OUTPUT);
  pinMode(IN4 + 4, OUTPUT);

  digitalWrite(3, HIGH);

//    Serial.println(resolutionX);
//    Serial.println(resolutionY);
}

const int limit = 400;
const int limitLow = 1023 / 2 - limit;
const int limitHigh = 1023 / 2 + limit;

void loop()
{

  int y = analogRead(1);
  int x = analogRead(2);

  if (x > limitHigh) {
    step(X, 100);
  }
  if (x < limitLow) {
    step(X, -100);
  }
  if (y > limitHigh) {
    step(Y, 100);
  }
  if (y < limitLow) {
    step(Y, -100);
  }


  //  int lvl = analogRead(3);
  //  Serial.println(lvl);


  if (digitalRead(3) == LOW) {

    boolean isForward = true;
    for (int i = 0; i < resolutionY; i++) {
      for (int j = 0; j < resolutionX; j++) {
        byte level = map(analogRead(3), 0, 1023, 0, 255);

        boolean isLastOnLine = j < (resolutionX - 1);
        if (isForward) {
          line[j] = level;
          if (isLastOnLine) {
            step(X, stepsXPerStep);
          }
        } else {
          line[resolutionX - 1 - j] = level;
          if (isLastOnLine) {
            step(X, -stepsXPerStep);
          }
        }
      }
      Serial.write(line, resolutionX);
      isForward = !isForward;
      step(Y, stepsYPerStep);
    }
    if (!isForward) {
      step(X, -(stepsXPerStep * resolutionX));
    }
    step(Y, -(stepsYPerStep * resolutionY));
  }
}

void step(int motor, int xw) {
  for (int i = 0; i < abs(xw); i++) {
    stepper(motor, xw > 0 ? 1 : -1);
  }
}

void stepper(int motor, int xw) {
  int steps = 0;
  switch (motor) {
    case 0:
      DirectionX = xw < 0; // compensating
      steps = StepsX;
      break;
    case 1:
      DirectionY = xw > 0;
      steps = StepsY;
      break;
  }
  xw = abs(xw);
  int pinOffset = motor * 4;
  for (int x = 0; x < xw; x++) {
    delayMicroseconds(1200);
    switch (steps) {
      case 0:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, HIGH);
        break;
      case 1:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, HIGH);
        digitalWrite(IN4 + pinOffset, HIGH);
        break;
      case 2:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, HIGH);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
      case 3:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, HIGH);
        digitalWrite(IN3 + pinOffset, HIGH);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
      case 4:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, HIGH);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
      case 5:
        digitalWrite(IN1 + pinOffset, HIGH);
        digitalWrite(IN2 + pinOffset, HIGH);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
      case 6:
        digitalWrite(IN1 + pinOffset, HIGH);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
      case 7:
        digitalWrite(IN1 + pinOffset, HIGH);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, HIGH);
        break;
      default:
        digitalWrite(IN1 + pinOffset, LOW);
        digitalWrite(IN2 + pinOffset, LOW);
        digitalWrite(IN3 + pinOffset, LOW);
        digitalWrite(IN4 + pinOffset, LOW);
        break;
    }
    SetDirection(motor);
  }
}

void SetDirection(int motor) {
  switch (motor) {
    case 0:
      if (DirectionX == 1) {
        StepsX++;
      }
      if (DirectionX == 0) {
        StepsX--;
      }
      if (StepsX > 7) {
        StepsX = 0;
      }
      if (StepsX < 0) {
        StepsX = 7;
      }
      break;
    case 1:
      if (DirectionY == 1) {
        StepsY++;
      }
      if (DirectionY == 0) {
        StepsY--;
      }
      if (StepsY > 7) {
        StepsY = 0;
      }
      if (StepsY < 0) {
        StepsY = 7;
      }
      break;
  }
}
