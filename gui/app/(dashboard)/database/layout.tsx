import { Suspense } from 'react';
import Loading from './loading';
import AsyncTree from './tree';

export default async function DatabaseLayout({
  searchParams,
  children
}: {
  searchParams: { q: string; offset: string };
  children: React.ReactNode;
}) {
  const search = searchParams?.q ?? '';
  const offset = searchParams?.offset ?? 0;

  return (
    <div className="flex divide-x h-full">
      <section className="w-1/4">
        <AsyncTree></AsyncTree>
      </section>
      <section className="flex-1 text-center">
        <Suspense fallback={<Loading></Loading>}>{children}</Suspense>
      </section>
    </div>
  );
}
