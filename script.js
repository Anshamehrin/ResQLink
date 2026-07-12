function showHome(){
    home.style.display='block';
    emergency.style.display='none';
}

function triggerSOS(){

    eName.textContent = name.value || '—';
    eBlood.textContent = blood.value;
    eAllergy.textContent = allergy.value || '—';
    eCondition.textContent = condition.value || '—';
    ePhone.textContent = phone.value || '—';

    gpsText.innerHTML =
        '<span class="k">LAT</span> ' + lat.textContent +
        '<br><span class="k">LON</span> ' + lon.textContent;

    alertTime.textContent = new Date().toLocaleString();

    home.style.display='none';
    emergency.style.display='block';
}
/* ---------- GPS Live Tracking ---------- */
var locationHistory=[];
function updateMapFrame(iframeEl, latitude, longitude){

    if(!iframeEl) return;

    iframeEl.src =
    "https://www.google.com/maps?q="+
    latitude+","+longitude+
    "&output=embed";
}
function logLocation(latitude, longitude, accuracy){

    var time = new Date().toLocaleTimeString();

    locationHistory.unshift(
        '<div class="history-line">' +
        time +
        ' — ' +
        latitude.toFixed(5) +
        ', ' +
        longitude.toFixed(5) +
        ' (' +
        Math.round(accuracy) +
        'm)</div>'
    );

    if(locationHistory.length > 8)
        locationHistory.pop();

    document.getElementById("locHistory").innerHTML =
        locationHistory.join("");
}
if(navigator.geolocation){
navigator.geolocation.watchPosition(function(pos){
var latitude=pos.coords.latitude;
var longitude=pos.coords.longitude;
var accuracy=pos.coords.accuracy;
lat.textContent=latitude.toFixed(6);
lon.textContent=longitude.toFixed(6);
acc.textContent='±'+Math.round(accuracy)+'m';
lastUpdate.textContent=new Date().toLocaleTimeString();
gpsStatus.textContent='LIVE';
gpsStatus.classList.remove('warn');
var gpsStatus2El=document.getElementById('gpsStatus2');
if(gpsStatus2El){gpsStatus2El.textContent='LIVE';gpsStatus2El.classList.remove('warn');}
maps.href='https://maps.google.com/?q='+latitude+','+longitude;
updateMapFrame(mapFrame,latitude,longitude);
updateMapFrame(document.getElementById('mapFrame2'),latitude,longitude);
var gpsTextEl=document.getElementById('gpsText');
if(gpsTextEl){

    gpsTextEl.innerHTML =
        "<span class='k'>LAT</span> " +
        latitude.toFixed(6) +
        "<br><span class='k'>LON</span> " +
        longitude.toFixed(6);

}
logLocation(latitude, longitude, accuracy);
},function(err){
gpsStatus.textContent=(err.code===1?'DENIED':'UNAVAILABLE');
gpsStatus.classList.add('warn');
},{enableHighAccuracy:true,maximumAge:2000,timeout:10000});
}else{
gpsStatus.textContent='UNSUPPORTED';
gpsStatus.classList.add('warn');
}
function receiveData(event){

    const value = new TextDecoder().decode(event.target.value);

    console.log(value);

    const parts = value.split("|");

    if(parts[0] === "SOS"){

        const lat = parts[1];
        const lon = parts[2];

        document.getElementById("lat").textContent = lat;
        document.getElementById("lon").textContent = lon;

        document.getElementById("gpsStatus").textContent = "CONNECTED";

        document.getElementById("maps").href =
            `https://www.google.com/maps?q=${lat},${lon}`;

        document.getElementById("mapFrame").src =
            `https://www.google.com/maps?q=${lat},${lon}&output=embed`;

        triggerSOS();
    }
}
let characteristic;

async function connectBLE(){

    const device = await navigator.bluetooth.requestDevice({

        filters:[
            {name:'ResQLink-Badge'}
        ],

        optionalServices:[
            '4fafc201-1fb5-459e-8fcc-c5c9c331914b'
        ]

    });

    const server = await device.gatt.connect();

    const service = await server.getPrimaryService(
        '4fafc201-1fb5-459e-8fcc-c5c9c331914b'
    );

    characteristic = await service.getCharacteristic(
        'beb5483e-36e1-4688-b7f5-ea07361b26a8'
    );

    await characteristic.startNotifications();

    characteristic.addEventListener(
        'characteristicvaluechanged',
        receiveData
    );

    alert("Badge Connected!");
}
window.onload = function(){

    connectBLE().catch(function(err){

        console.log(err);

    });

}