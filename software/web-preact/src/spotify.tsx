import SpotifyCredsComp from './components/spotify_creds';

const SpotifyPage = () => {

  return (
    <div>
        <p style={{ textAlign: 'center', marginTop: 0, fontSize: '0.9rem' }}>
          Spotify API Credentials Setup
        </p>
        <SpotifyCredsComp />
    </div>
  );
}

export default SpotifyPage;