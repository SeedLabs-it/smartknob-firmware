import { render } from 'preact';
import SetupPage from './setup';

import WiFiComp from './components/wifi';
import MqttComp from './components/mqtt';
import DoneComp from './components/done';
import SpotifyCredsComp from './components/spotify_creds';

import StatusAlert from 'react-status-alert';
import 'react-status-alert/dist/status-alert.css';
import './style.scss';
import { useEffect, useState } from 'preact/hooks';

export function App() {
  const [spotify, setSpotify] = useState(undefined);
  const [wifi, setWifi] = useState(false);
  const [mqtt, setMqtt] = useState(false);
  const [done, setDone] = useState(false);

  useEffect(() => {
    const urlParams = new URLSearchParams(window.location.search);
    if (urlParams.has('spotify')) {
      setSpotify(true);
    } else {
      setSpotify(false);
    }

    if (urlParams.has('wifi')) {
      setWifi(true);
    } else {
      setWifi(false);
    }

    if (urlParams.has('mqtt')) {
      setMqtt(true);
    } else {
      setMqtt;
    }

    if (urlParams.has('done')) {
      setDone(true);
    } else {
      setDone(false);
    }
  }, []);

  if (spotify === undefined) {
    return <div>Loading...</div>;
  }

  return (
    <div>
      <StatusAlert />
      <h1 style={{ textAlign: 'center', margin: 0 }}>SMARTKNOB DEV KIT</h1>
      {/* {setup ? <SetupPage /> : <SpotifyPage />} */}
      {!spotify && !wifi && !mqtt && !done && (
        <>
          <div>default</div>
        </>
      )}
      {wifi && <WiFiComp />}
      {mqtt && <MqttComp />}
      {spotify && <SpotifyCredsComp />}
      {done && <DoneComp />}
    </div>
  );
}

render(<App />, document.getElementById('app'));
