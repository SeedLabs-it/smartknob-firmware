import { render } from 'preact';
import WiFiComp from './components/wifi';

import preactLogo from './assets/preact.svg';
import './style.css';
import { useState } from 'preact/hooks';
import MQTTComp from './components/mqtt';
import DoneComp from './components/done';

const SetupPage = () => {
  const [activeTab, setActiveTab] = useState('wifi');

  return (
    <>
      <div>
        <p style={{ textAlign: 'center', marginTop: 0, fontSize: '0.9rem' }}>
          WiFi & MQTT Setup
        </p>
        <div style={{ marginBottom: 8 }}>
          <button
            style={{ width: '50%' }}
            className={activeTab === 'wifi' ? 'active' : ''}
            onClick={() => setActiveTab('wifi')}
          >
            WiFi
          </button>
          <button
            style={{ width: '50%' }}
            className={activeTab === 'mqtt' ? 'active' : ''}
            onClick={() => setActiveTab('mqtt')}
          >
            MQTT
          </button>
        </div>
        <div className='tab-content'>
          {activeTab === 'wifi' && <WiFiComp setActiveTab={setActiveTab} />}
          {activeTab === 'mqtt' && <MQTTComp setActiveTab={setActiveTab} />}
          {activeTab === 'done' && ( // Done tab
            <DoneComp />
          )}
        </div>
      </div>
      <br />
      <div id='download-btn'>
        <a href={`http://${window.location.hostname}/download/config`} download>
          <button>Download Config</button>
        </a>
      </div>
    </>
  );
}

export default SetupPage;