/*
using UnityEngine;
using System.Collections;
using System.IO.Ports;

public class ReadViaUSB : MonoBehaviour {
    SerialPort sp = new SerialPort("COM3", 9600);
    // SerialPort sp = new SerialPort("/dev/ttyUSB0", 9600); // on linux

    void Start () {
        sp.Open ();
        sp.ReadTimeout = 1;
    }

    void Update () {
        try {
            //print (sp.ReadLine());
            string[] values = sp.ReadLine().Split(',');
            b1 = (float.Parse(values[0]));
            print (b1);
            b2 = (float.Parse(values[1]));
            print (b2);
        }
        catch(System.Exception) {
        }
    }
}
*/