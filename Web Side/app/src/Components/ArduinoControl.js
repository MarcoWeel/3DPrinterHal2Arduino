import React, { useEffect } from "react";
import JSMpeg from "@cycjimmy/jsmpeg-player";
import axios from "axios";

const ffmpegIP = "localhost";

const handleStopClick = () => {
    if (window.confirm("Are you sure want to stop the printer?")) {
      axios({
        method: "post",
        url: "http://192.168.2.33/stop/" ,
      })
        .then((res) => {
          console.log(res);
          console.log(res.data);
        })
        .catch((error) => {
          console.log(error.response);
          console.log(error);
        });
    }
  };

const handlePauseClick = () => {
    if (window.confirm("Are you sure want to pause the printer?")) {
      axios({
        method: "post",
        url: "http://192.168.2.33/pause/" ,
      })
        .then((res) => {
          console.log(res);
          console.log(res.data);
        })
        .catch((error) => {
          console.log(error.response);
          console.log(error);
        });
    }
  };

const ArduinoControl = () => {
  
  return (
    <div id="body">
      <button onClick={handleStopClick}>Stop</button>
      <button onClick={handlePauseClick}>Pause</button>
    </div>
  );
};

export default ArduinoControl;
