import { Component } from 'preact';
import { useRef, useState } from 'preact/hooks';

import './comp.css';
import { StatusAlertService } from 'react-status-alert';

const SpotifyCredsComp = () => {
  const [disabled, setDisabled] = useState(false);

  const [clientId, setClientId] = useState('');
  const [clientSecret, setClientSecret] = useState('');

  const [status, setStatus] = useState('');

  // Handle form submission
  const handleSubmit = async (e) => {
    e.preventDefault();

    const data = { clientId, clientSecret };

    try {
      setDisabled(true);
      const response = await fetch(`http://${window.location.hostname}/spotify`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
      });

      if (response.ok) {
        StatusAlertService.showSuccess(
          <div>
            <h3>Spotify Connected</h3>
          </div>,
          {
            autoHide: true,
            withIcon: false,
            withCloseIcon: false,
            removeAllBeforeShow: true,
            autoHideTime: 2500,
          },
        );
      } else {
        StatusAlertService.showWarning(
          <div>
            <h3>Provided Spotify credentials did not work.</h3>
          </div>,
          {
            autoHide: true,
            withIcon: false,
            withCloseIcon: false,
            removeAllBeforeShow: true,
            autoHideTime: 5000,
          },
        );
      }
    } catch (error) {
      StatusAlertService.showError(
        <div>
          <h3>No response from knob.</h3>
        </div>,
        {
          autoHide: true,
          withIcon: false,
          withCloseIcon: false,
          removeAllBeforeShow: true,
          autoHideTime: 5000,
        },
      );
    }
    setDisabled(false);
  };

  return (
    <div className='comp'>
      <h1>SPOTIFY</h1>
      <form onSubmit={handleSubmit}>
        <label>
          CLIENT ID
          <input
            type='text'
            value={clientId}
            onInput={(e) => setClientId(e.currentTarget.value)}
            required
          />
        </label>
        <br />
        <label>
          CLIENT SECRET
          <input
            type='text'
            value={clientSecret}
            onInput={(e) => setClientSecret(e.currentTarget.value)}
            required
          />
        </label>
        <br />
        <button type='submit' disabled={disabled}>
          Submit
        </button>
      </form>
      {status && <p>{status}</p>}
    </div>
  );
};

export default SpotifyCredsComp;
