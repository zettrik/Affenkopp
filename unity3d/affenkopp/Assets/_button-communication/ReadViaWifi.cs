using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
 
public class ReadViaWifi : MonoBehaviour {
    void Start() {
        StartCoroutine(GetText());
        Debug.unityLogger.Log("hello");
    }
 
    IEnumerator GetText() {
        UnityWebRequest www = UnityWebRequest.Get("http://192.168.1.1/buttons");
        yield return www.Send();
 
        if(www.isNetworkError) {
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
