'use client';

import { Spinner } from '@/components/icons';
import { Input } from '@/components/ui/input';
import { Search } from 'lucide-react';
import { useRouter } from 'next/navigation';
import { useTransition } from 'react';

export function SearchInput() {
  const router = useRouter();
  const [isPending, startTransition] = useTransition();

  function searchAction(formData: FormData) {
    const value = formData.get('q') as string;
    const params = new URLSearchParams({ q: value });
    startTransition(() => {
      router.replace(`/?${params.toString()}`);
    });
  }

  return (
    <form action={searchAction} className="relative ml-auto flex-1 md:grow-0">
      <Search className="absolute left-2.5 top-[.75rem] h-4 w-4 text-muted-foreground" />
      <Input
        name="q"
        type="search"
        placeholder="Search..."
        className="w-full rounded-lg bg-background pl-8 md:w-[200px] lg:w-[336px]"
      />
      {isPending && <Spinner />}
    </form>
  );
}
