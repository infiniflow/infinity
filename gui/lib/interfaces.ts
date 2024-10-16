export interface IDialogProps<T> {
  showDialog?(): void;
  hideDialog(): void;
  switchVisible(): void;
  visible?: boolean;
  loading?: boolean;
  onOk?(payload?: T): Promise<unknown> | void;
}
