import { useEffect, useState } from 'preact/hooks';
import { showError, showSuccess } from './status-alerts';

const generateRandomString = (length) => {
  const possible =
    'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const randomValues = crypto.getRandomValues(new Uint8Array(length));
  const randomString = randomValues.reduce(
    (acc, x) => acc + possible[x % possible.length],
    '',
  );
  return randomString;
};

const getSpotifyOAuthUrl = async (clientId) => {
  var redirectUrl = `http://${window.location.hostname}/?spotify`;
  if (window.location.hostname === 'localhost') {
    redirectUrl = `http://${window.location.hostname}:5173/?spotify`;
  }
  const scope = 'streaming user-modify-playback-state user-read-playback-state';

  const state = generateRandomString(16);

  const authUrl = new URL('https://accounts.spotify.com/authorize');
  const params = {
    response_type: 'code',
    client_id: clientId,
    scope: scope,
    redirect_uri: redirectUrl,
    state: state,
  };

  authUrl.search = new URLSearchParams(params).toString();

  return { authUrl, state };
};

const getAccessToken = async (code, client_id, client_secret) => {
  var redirectUrl = `http://${window.location.hostname}/?spotify`;
  if (window.location.hostname === 'localhost') {
    redirectUrl = `http://${window.location.hostname}:5173/?spotify`;
  }

  const response = await fetch('https://accounts.spotify.com/api/token', {
    method: 'POST',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
      Authorization: 'Basic ' + btoa(client_id + ':' + client_secret),
    },
    body: new URLSearchParams({
      code: code,
      redirect_uri: redirectUrl,
      grant_type: 'authorization_code',
    }).toString(),
  });

  if (response.ok) {
    const data = await response.json();
    return data;
  } else {
    return undefined;
  }
};

type SpotifyAuthResponse = {
  access_token: string;
  token_type: string;
  scope: string;
  expires_in: number;
  refresh_token: string;

  timestamp: number;
};

const SpotifyPage = () => {
  const [clientId, setClientId] = useState('');
  const [clientSecret, setClientSecret] = useState('');

  const [authUrl, setAuthUrl] = useState('');
  const [authState, setAuthState] = useState('');
  const [storedState, setStoredState] = useState('');

  const [code, setCode] = useState('');
  const [state, setState] = useState('');

  const [authResponse, setAuthResponse] = useState({} as SpotifyAuthResponse);

  useEffect(() => {
    if (window.location.search.includes('code')) {
      setCode(new URLSearchParams(window.location.search).get('code'));
    }

    if (window.location.search.includes('state')) {
      setAuthState(new URLSearchParams(window.location.search).get('state'));
    }

    if (localStorage.getItem('spotify_auth_state')) {
      setStoredState(localStorage.getItem('spotify_auth_state'));
    }

    if (localStorage.getItem('client_id')) {
      setClientId(localStorage.getItem('client_id'));
    }

    if (localStorage.getItem('client_secret')) {
      setClientSecret(localStorage.getItem('client_secret'));
    }
  }, []);
  return (
    <div>
      <p style={{ textAlign: 'center', marginTop: 0, fontSize: '0.9rem' }}>
        Spotify API Credentials Setup
      </p>
      <div className='comp'>
        <h1>SPOTIFY</h1>
        <form action=''>
          <label>
            CLIENT ID
            <input
              type='text'
              value={clientId}
              onInput={(e) => setClientId(e.currentTarget.value)}
              required
              disabled={authState != '' && authState === storedState}
            />
          </label>
          <br />
          <a
            className={`btn ${authState != '' && authState === storedState ? 'disabled' : ''}`}
            href='#'
            onClick={async (e) => {
              e.preventDefault();

              localStorage.setItem('client_id', clientId);
              localStorage.setItem('client_secret', clientSecret);

              const { authUrl, state: state_ } =
                await getSpotifyOAuthUrl(clientId);
              setAuthUrl(authUrl.toString());
              setState(state_);
              localStorage.setItem('spotify_auth_state', state_);
              window.location.href = authUrl.toString();
            }}
          >
            LOGIN TO SPOTIFY
          </a>
        </form>
        <form
          onSubmit={async (e) => {
            e.preventDefault();

            localStorage.setItem('client_id', clientId);
            localStorage.setItem('client_secret', clientSecret);

            const data = await getAccessToken(code, clientId, clientSecret);

            if (data === undefined) {
              console.log('Couldnt get access token.');
              showError('Couldnt get access token.');
              localStorage.setItem('spotify_auth_state', '');
              setStoredState('');
              return;
            }

            data.timestamp = Date.now() - 1000;
            // setAuthResponse(data as SpotifyAuthResponse);
            showSuccess('Spotify Connected');
            localStorage.setItem('spotify_auth_state', '');
            setStoredState('');

            const response = await fetch(
              window.location.hostname === 'localhost'
                ? `http://${window.location.hostname}:5173/spotify`
                : `http://${window.location.hostname}/spotify`,
              {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                  base64_id_and_secret: btoa(
                    '92e0c94d48174b608ded52e5a5a26aa0' +
                      ':' +
                      '72e8283140e2415c96f4bd66a76ced0b',
                  ),
                  spotify_response: data as SpotifyAuthResponse,
                }),
              },
            );

            if (response.ok) {
              console.log('Spotify Details Stored Successfully');
              showSuccess('Spotify Details Stored Successfully');
            } else {
              console.log('Failed to store Spotify Details');
              showError('Failed to store Spotify Details');
            }
          }}
          className={`${authState == '' || authState !== storedState ? 'disabled' : ''}`}
        >
          <br />
          <label>
            CLIENT SECRET
            <input
              type='text'
              value={clientSecret}
              onInput={(e) => setClientSecret(e.currentTarget.value)}
              required
              disabled={authState == '' || authState !== storedState}
            />
          </label>
          <br />
          <button type='submit' disabled={authState !== storedState}>
            RETRIEVE & SUBMIT ACCESS TOKEN
          </button>
        </form>
        {status && <p>{status}</p>}
      </div>
    </div>
  );
};

export default SpotifyPage;
