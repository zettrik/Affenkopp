using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
 
public class ReadViaWifi : MonoBehaviour {
    void Start() {
        StartCoroutine(GetText());
    }
 
    IEnumerator GetText() {
        UnityWebRequest www = UnityWebRequest.Get("http://172.16.0.135/buttons");
        yield return www.Send();
 
        if(www.isError) {
            Debug.Log(www.error);
        }
        else {
            // Show results as text
            Debug.Log(www.downloadHandler.text);
 
            // Or retrieve results as binary data
            //byte[] results = www.downloadHandler.data;
        }
    }
}
