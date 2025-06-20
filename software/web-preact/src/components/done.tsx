import './comp.css';

import { useEffect, useState } from 'preact/hooks';

const DoneComp = () => {
  const [urlParams, setUrlParams] = useState(new URLSearchParams());
  useEffect(() => {
    const urlParams = new URLSearchParams(window.location.search);
    setUrlParams(urlParams);
  }, []);
  return (
    <div>
      <div style={{ marginTop: '24px' }}>
        {urlParams.has('mqtt') && (
          <>
            <h2>On device setup complete</h2>
            <p>Continue in ur homeassistant instance.</p>
          </>
        )}
        {urlParams.has('spotify') && (
          <>
            <h2>Spotify setup done</h2>
            <p>Continue on device.</p>
          </>
        )}
      </div>
    </div>
  );
};

export default DoneComp;
