import { render } from 'preact';
import SetupPage from './setup';
import SpotifyPage from './spotify';

import StatusAlert from 'react-status-alert';
import 'react-status-alert/dist/status-alert.css';
import './style.scss';
import { useEffect, useState } from 'preact/hooks';

export function App() {
  const [setup, setSetup] = useState(undefined);
  useEffect(() => {
    const urlParams = new URLSearchParams(window.location.search);
    if (urlParams.has('spotify')) {
      setSetup(false);
    } else {
      setSetup(true);
    }
  }, []);

  if (setup === undefined) {
    return <div>Loading...</div>;
  }

  return (
    <div>
      <StatusAlert />
      <h1 style={{ textAlign: 'center', margin: 0 }}>SMARTKNOB DEV KIT</h1>

      {setup ? <SetupPage /> : <SpotifyPage />}
    </div>
  );
}

render(<App />, document.getElementById('app'));
