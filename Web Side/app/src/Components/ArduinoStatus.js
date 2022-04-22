import React, { useEffect, useState } from "react";
import axios from "axios";

function ArduinoStatus() {
  const [isOnline, setOnlineStatus] = useState(false);
  axios
    .get("http://84.86.167.197:8083/online")
    .then((response) => {
      console.log("TEST");
      setOnlineStatus(true);
    })
    .catch((error) => {
      setOnlineStatus(false);
    });
  const url = "http://84.86.167.197:8083/temp";
  const [request, setRequest] = useState({
    loading: false,
    data: null,
    error: false,
  });
  const GetStatus = async () => {
    try {
      setRequest({
        loading: true,
        data: null,
        error: false,
      });
      const Status = await axios.get(url);
      setRequest({
        loading: false,
        data: Status.data,
        error: false,
      });
    } catch (err) {
      setRequest({
        loading: false,
        data: null,
        error: true,
      });
    }
  };

  useEffect(() => {
    const interval = setInterval(() => {
      GetStatus();
      console.log(request.data);
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div>
      {isOnline && (
        <div>
          <span>Head temperature</span>
          <span>{request.data.headTemp}</span>
          <span>Bed temperature</span>
          <span>{request.data.bedTemp}</span>
          <span>Time remaining</span>
          <span>{request.data.timeRemaining}</span>
          {request.data.printerStatus == 0 && (
            <div>
              <span>Printer status : Offline</span>
            </div>
          )}
          {request.data.printerStatus == 1 && (
            <div>
              <span>Printer status : Online</span>
            </div>
          )}
          {request.data.programStatus == 0 && (
            <div>
              <span>Printer is not running</span>
            </div>
          )}
          {request.data.programStatus == 1 && (
            <div>
              <span>Program is running</span>
            </div>
          )}
        </div>
      )}
    </div>
  );
}

export default ArduinoStatus;
