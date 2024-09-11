// import { getProducts } from '@/lib/db';

import { ApiUrl } from '@/lib/constant/api';
import { request } from '@/lib/utils';

export default async function HomePage({
  searchParams
}: {
  searchParams: { q: string; offset: string };
}) {
  const x = await request(ApiUrl.databases);
  console.log('🚀 ~ x:', x);
  const search = searchParams.q ?? '';
  const offset = searchParams.offset ?? 0;

  return <div>HomePage</div>;
}
