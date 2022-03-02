import { useAxiosGet } from "../Hooks/HttpRequest";
function ArduinoStatus() {
const url = "http://localhost:5006/Floor/";
let status = useAxiosGet(url);
return(
<div>
    <div>
        <span>Head temperature</span>
        <span>{status.data.headTemp}</span>
        <span>Bed temperature</span>
        <span>{status.data.bedTemp}</span>
        <span>Time remaining</span>
        <span>{status.data.timeLeft}</span>
        <span>Duration</span>
        <span>{status.data.timeDone}</span>
    </div>
</div>
);

}

export default ArduinoStatus;