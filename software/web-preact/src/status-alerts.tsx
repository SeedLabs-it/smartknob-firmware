import { StatusAlertService } from 'react-status-alert';

const showSuccess = (message: string) => {
  StatusAlertService.showSuccess(
    <div>
      <h3>{message}</h3>
    </div>,
    {
      autoHide: true,
      withIcon: false,
      withCloseIcon: false,
      removeAllBeforeShow: true,
      autoHideTime: 5000,
    },
  );
};

const showWarning = (message: string) => {
  StatusAlertService.showWarning(
    <div>
      <h3>{message}</h3>
    </div>,
    {
      autoHide: true,
      withIcon: false,
      withCloseIcon: false,
      removeAllBeforeShow: true,
      autoHideTime: 3000,
    },
  );
};

const showError = (message: string) => {
  StatusAlertService.showError(
    <div>
      <h3>{message}</h3>
    </div>,
    {
      autoHide: true,
      withIcon: false,
      withCloseIcon: false,
      removeAllBeforeShow: true,
      autoHideTime: 3000,
    },
  );
};

export { showSuccess, showWarning, showError };
