import { Component } from 'preact';
import { useRef, useState } from 'preact/hooks';

import './comp.css';
import { StatusAlertService } from 'react-status-alert';

const MqttComp = () => {
  const [disabled, setDisabled] = useState(false);

  const [server, setServer] = useState('');
  const [port, setPort] = useState('');
  const [username, setUsername] = useState('');
  const [password, setPswd] = useState('');

  const [status, setStatus] = useState('');

  // Handle form submission
  const handleSubmit = async (e) => {
    e.preventDefault();
    const portInt = parseInt(port);
    const data = { server, portInt, username, password };

    try {
      setDisabled(true);
      const response = await fetch(`http://${window.location.hostname}/mqtt`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
      });

      if (response.ok) {
        StatusAlertService.showSuccess(
          <div>
            <h3>MQTT Connected</h3>
          </div>,
          {
            autoHide: true,
            withIcon: false,
            withCloseIcon: false,
            removeAllBeforeShow: true,
            autoHideTime: 2500,
          },
        );

        var json = await response.json();
        window.location.href = '/?' + json.redirect + '&mqtt';
      } else {
        StatusAlertService.showWarning(
          <div>
            <h3>MQTT Couldnt connect, please check provided data</h3>
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
      <h1>MQTT</h1>
      <form onSubmit={handleSubmit}>
        <label>
          SERVER
          <input
            type='text'
            value={server}
            onInput={(e) => setServer(e.currentTarget.value)}
            required
          />
        </label>
        <br />
        <label>
          PORT
          <input
            type='text'
            value={port}
            onInput={(e) => setPort(e.currentTarget.value)}
            required
          />
        </label>
        <br />
        <label>
          USERNAME
          <input
            type='text'
            value={username}
            onInput={(e) => setUsername(e.currentTarget.value)}
          />
        </label>
        <br />
        <label>
          PASSWORD
          <input
            type='password'
            value={password}
            onInput={(e) => setPswd(e.currentTarget.value)}
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

export default MqttComp;
