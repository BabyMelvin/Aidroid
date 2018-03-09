package melvin.hardware;

interface ILedService{
    boolean setOn(int led);
    boolean setOff(int led);
    booelan setAllOn();
    boolean setName(String name);
}
