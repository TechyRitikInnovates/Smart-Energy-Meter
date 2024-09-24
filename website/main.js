const firebaseConfig = {
  apiKey: "AIzaSyCu1mQApkRU9k_m_GdpUKkfmAXCTDQpPfw",
  authDomain: "energy-meter-2deb1.firebaseapp.com",
  databaseURL: "https://energy-meter-2deb1-default-rtdb.firebaseio.com",
  projectId: "energy-meter-2deb1",
  storageBucket: "energy-meter-2deb1.appspot.com",
  messagingSenderId: "414645995276",
  appId: "1:414645995276:web:aaa60bb0b9ae58f6a22b29"
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Reference to the Realtime Database
const database = firebase.database();

// Reference to the sensor data (entire PZEM-400t object)
const sensorRef = database.ref('sensorData/');

// Fetch and update data in real-time
sensorRef.on('value', (snapshot) => {
  const data = snapshot.val(); // Get the data object from Firebase
  
  if (data) {
    // Update the values on the web page
    document.getElementById('voltage').innerText = data.voltage || '--';
    document.getElementById('current').innerText = data.current || '--';
    document.getElementById('power').innerText = data.power || '--';
    document.getElementById('energy').innerText = data.energy || '--';
    document.getElementById('freq').innerText = data.frequency || '--';
    document.getElementById('pf').innerText = data.pf || '--';
  } else {
    console.log('No data available');
  }
});