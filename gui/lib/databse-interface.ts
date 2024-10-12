export type IResponseBody<T extends any> = {
  error_code: number;
} & { [U: string | number]: T };

export interface ITableColumns {
  name: string;
  type: string;
  default: any;
}

export interface ITableIndex {
  columns: string;
  index_name: string;
  index_type: string;
}

export interface ITableSegment {
  id: string;
  size: string;
  status: string;
}
