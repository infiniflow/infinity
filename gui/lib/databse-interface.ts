export interface IResponseBody<T> {
  error_code: number;
  [U: any]: T;
}

export interface ITableColumns {
  name: string;
  type: string;
  default: any;
}
