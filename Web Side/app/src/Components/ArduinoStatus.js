import React, { useEffect} from "react";
import axios from "axios";

function ArduinoStatus() {
    const url = "http://192.168.2.33/temp/";
    let status = null;
    useEffect(() => {
        const interval = setInterval(() => {
           status = axios.get(url);
        }, 1000);
        return () => clearInterval(interval);
      }, []);

return(
<div>
    {status != null &&
    <div>
        <span>Head temperature</span>
        <span>{status.headTemp}</span>
        <span>Bed temperature</span>
        <span>{status.bedTemp}</span>
        <span>Time remaining</span>
        <span>{status.timeLeft}</span>
        <span>Duration</span>
        <span>{status.timeDone}</span>
    </div>}
</div>
);

}

export default ArduinoStatus;