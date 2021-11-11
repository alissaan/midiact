#include <Bounce2.h>
#include <MIDIUSB.h>

const uint8_t _channelCount = 3;
const bool _useAbsoluteValues = false;
int _currentChannel = 0;

struct ButtonController
{
  bool IsEnabled = false;
  uint8_t Type = 0; //0 = Normal button, 9 = Channel shift button
  uint8_t Port = 0;
  uint8_t Parameter;
  uint8_t Status = 0;
  bool HasFell = true;
  Bounce2::Button Device = Bounce2::Button();

public:
  ButtonController(uint8_t type, uint8_t port, uint8_t parameter)
  {
    Type = type;
    Port = port;
    Parameter = parameter;
  }
};

ButtonController b0 = ButtonController(0, 13, 3);
ButtonController b1 = ButtonController(9, 0, 0);
ButtonController b2 = ButtonController(0, 12, 22);
ButtonController b3 = ButtonController(0, 11, 23);
ButtonController b4 = ButtonController(0, 1, 24);

const uint8_t _buttonCount = 5;
ButtonController _commonButtons[_buttonCount] = {b0, b1, b2, b3, b4};

void setup()
{
  b0.HasFell = false;

  for (int i = 0; i < _buttonCount; i++)
  {
    _commonButtons[i].Device.attach(_commonButtons[i].Port, INPUT_PULLUP);
  }
}

void loop()
{
  for (int i = 0; i < _buttonCount; i++)
  {
    _commonButtons[i].Device.update();

    if (_commonButtons[i].Type == 9)
    {
      if (_commonButtons[i].Device.pressed())
      {
        _currentChannel++;

        if (_currentChannel > _channelCount)
        {
          _currentChannel = 0;
        }

        printChannel();
      }
    }
    else if (_commonButtons[i].Type == 0)
    {
      if (_commonButtons[i].Device.pressed() || (_commonButtons[i].HasFell && _commonButtons[i].Device.fell()))
      {
        _commonButtons[i].Status = !_commonButtons[i].Status;

        uint8_t velocity = _commonButtons[i].Status == HIGH ? 127 : 0;

        if (_useAbsoluteValues)
        {
          velocity = 127;
        }

        uint8_t param = _commonButtons[i].Parameter;

        if (param > 0)
        {
          midiEventPacket_t event = {0x0B, 0xB0 | _currentChannel, param, velocity};
          MidiUSB.sendMIDI(event);
          MidiUSB.flush();
          String message = (String) "Channel: " + _currentChannel + " / Button: " + i + " / Parameter: " + param + " / Velocity: " + velocity;
          Serial.println(message);

          printChannel();
        }

        _currentChannel = 0;
      }
    }
  }
}

void printChannel()
{
  String message = (String) "Current channel: " + _currentChannel;
  Serial.println(message);
}
