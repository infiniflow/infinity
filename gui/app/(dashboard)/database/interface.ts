import { Leaf } from './constants';

export type TreeParentId = string | number | null;

export interface DatabaseRouteParams {
  params: { databaseId: string; tableId: string };
  searchParams: { tab: Leaf };
}
