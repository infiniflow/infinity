import { Suspense } from 'react';
import Loading from './loading';
import AsyncTree from './tree';

export default async function DatabaseLayout({
  children
}: {
  searchParams: { q: string; offset: string };
  children: React.ReactNode;
}) {
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
