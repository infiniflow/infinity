export type IResponseBody<T = Record<string, unknown>> = {
  error_code: number;
} & { [U: string | number]: T };

export interface ITableColumns {
  name: string;
  type: string;
  default: unknown;
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
