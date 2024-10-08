import {
  Collapsible,
  CollapsibleContent,
  CollapsibleTrigger
} from '@/components/ui/collapsible';
import Link from 'next/link';
import { ReactNode } from 'react';
import { ContextMenu, ContextMenuTrigger } from './context-menu';

function ChevronRightIcon(props: any) {
  return (
    <svg
      {...props}
      xmlns="http://www.w3.org/2000/svg"
      width="24"
      height="24"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
    >
      <path d="m9 18 6-6-6-6" />
    </svg>
  );
}

export interface SubItem {
  key: string;
  label: string;
}

export interface MenuItem {
  key: string;
  label: string;
  icon?: ReactNode;
  children: SubItem[];
}

interface IProps {
  items: MenuItem[];
  contextMenuContent?: ((key: string) => ReactNode) | ReactNode;
}

export default function SideMenu({ items, contextMenuContent }: IProps) {
  return (
    <div className="grid gap-2 py-6">
      <Collapsible className="grid gap-4">
        <CollapsibleTrigger className="flex w-full items-center text-lg font-semibold [&[data-state=open]>svg]:rotate-90">
          Home
          <ChevronRightIcon className="ml-auto h-5 w-5 transition-all" />
        </CollapsibleTrigger>
        <CollapsibleContent>
          <div className="-mx-6 grid gap-6 bg-gray-100 p-6 dark:bg-gray-800">
            <Link
              href="#"
              className="group grid h-auto w-full justify-start gap-1"
              prefetch={false}
            >
              <div className="text-sm font-medium leading-none group-hover:underline">
                Dashboard
              </div>
            </Link>
            <Link
              href="#"
              className="group grid h-auto w-full justify-start gap-1"
              prefetch={false}
            >
              <div className="text-sm font-medium leading-none group-hover:underline">
                Profile
              </div>
            </Link>
          </div>
        </CollapsibleContent>
      </Collapsible>
      <Collapsible className="grid gap-4">
        <CollapsibleTrigger className="flex w-full items-center text-lg font-semibold [&[data-state=open]>svg]:rotate-90">
          Products
          <ChevronRightIcon className="ml-auto h-5 w-5 transition-all" />
        </CollapsibleTrigger>
        <CollapsibleContent>
          <div className="-mx-6 grid gap-6 bg-gray-100 p-6 dark:bg-gray-800">
            <Link
              href="#"
              className="group grid h-auto w-full justify-start gap-1"
              prefetch={false}
            >
              <div className="text-sm font-medium leading-none group-hover:underline">
                View Products
              </div>
            </Link>
            <Link
              href="#"
              className="group grid h-auto w-full justify-start gap-1"
              prefetch={false}
            >
              <div className="text-sm font-medium leading-none group-hover:underline">
                Add Product
              </div>
            </Link>
          </div>
        </CollapsibleContent>
      </Collapsible>

      {items.map((x) => {
        return (
          <Collapsible className="grid gap-4" key={x.key}>
            <ContextMenu>
              <ContextMenuTrigger>
                <CollapsibleTrigger className="flex w-full items-center text-lg font-semibold [&[data-state=open]>svg]:rotate-90">
                  {x.label}
                  <ChevronRightIcon className="ml-auto h-5 w-5 transition-all" />
                </CollapsibleTrigger>
                {typeof contextMenuContent === 'function'
                  ? contextMenuContent(x.key)
                  : contextMenuContent}
              </ContextMenuTrigger>
            </ContextMenu>
            <CollapsibleContent>
              <div className="-mx-6 grid gap-6 bg-gray-100 p-6 dark:bg-gray-800">
                {x.children.map((y) => {
                  return (
                    <Link
                      href="#"
                      className="group grid h-auto w-full justify-start gap-1"
                      prefetch={false}
                      key={y.key}
                    >
                      <div className="text-sm font-medium leading-none group-hover:underline">
                        {y.label}
                      </div>
                    </Link>
                  );
                })}
              </div>
            </CollapsibleContent>
          </Collapsible>
        );
      })}
    </div>
  );
}
