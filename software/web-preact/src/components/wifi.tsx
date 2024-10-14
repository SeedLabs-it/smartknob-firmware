import { Component } from 'preact';
import { useRef, useState } from 'preact/hooks';

import './comp.css';
import { StatusAlertService } from 'react-status-alert';

const WiFiComp = ({ setActiveTab }: { setActiveTab: any }) => {
  const [disabled, setDisabled] = useState(false);

  const [ssid, setSsid] = useState('');
  const [password, setPswd] = useState('');

  const [status, setStatus] = useState('');

  // Handle form submission
  const handleSubmit = async (e) => {
    e.preventDefault();

    const data = { ssid, password };

    try {
      setDisabled(true);
      const response = await fetch(`http://${window.location.hostname}/wifi`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
      });

      if (response.ok) {
        StatusAlertService.showSuccess(
          <div>
            <h3>WiFi Connected</h3>
          </div>,
          {
            autoHide: true,
            withIcon: false,
            withCloseIcon: false,
            removeAllBeforeShow: true,
            autoHideTime: 2500,
          },
        );
        setActiveTab('mqtt');
      } else {
        StatusAlertService.showWarning(
          <div>
            <h3>WiFi Couldnt connect, please check provided credentials</h3>
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
      <h1>WiFi</h1>
      <form onSubmit={handleSubmit}>
        <label>
          SSID
          <input
            type='text'
            value={ssid}
            onInput={(e) => setSsid(e.currentTarget.value)}
            required
          />
        </label>
        <br />
        <label>
          PASSWORD
          <input
            type='password'
            value={password}
            onInput={(e) => setPswd(e.currentTarget.value)}
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

export default WiFiComp;
