import React, { useEffect, useState } from "react";
import axios from "axios";

function ArduinoStatus() {
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
      {request.data != null && (
        <div>
          <span>Head temperature</span>
          <span>{request.data.headTemp}</span>
          <span>Bed temperature</span>
          <span>{request.data.bedTemp}</span>
          {/* <span>Time remaining</span> */}
          {/* <span>{status.timeLeft}</span> */}
          {/* <span>Duration</span> */}
          {/* <span>{status.timeDone}</span> */}
        </div>
      )}
    </div>
  );
}

export default ArduinoStatus;
