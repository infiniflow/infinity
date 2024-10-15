'use client';

import { zodResolver } from '@hookform/resolvers/zod';
import { useRouter } from 'next/navigation';
import { useForm } from 'react-hook-form';
import { z } from 'zod';

import { toast } from '@/components/hooks/use-toast';
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage
} from '@/components/ui/form';
import { Input } from '@/components/ui/input';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue
} from '@/components/ui/select';
import { CreateOption } from '@/lib/constant/common';
import { createDatabase } from '../actions';

export const FormSchema = z.object({
  database_name: z
    .string({
      required_error: 'Please input name'
    })
    .trim(),
  // fields: z.array(),
  create_option: z.nativeEnum(CreateOption)
});

interface IProps {
  hide(): void;
}

export function TableCreatingForm({ hide }: IProps) {
  const router = useRouter();
  const form = useForm<z.infer<typeof FormSchema>>({
    resolver: zodResolver(FormSchema),
    defaultValues: {
      create_option: CreateOption.Error
    }
  });

  async function onSubmit(data: z.infer<typeof FormSchema>) {
    const ret = await createDatabase(data);
    console.log('🚀 ~ onSubmit ~ ret:', ret);
    if (ret.error_code === 0) {
      router.refresh();
      hide();
      toast({
        title: 'Create Success',
        description: ''
      });
    }
  }

  return (
    <Form {...form}>
      <form
        onSubmit={form.handleSubmit(onSubmit)}
        className="w-2/3 space-y-6"
        id="database-creating"
      >
        <FormField
          control={form.control}
          name="database_name"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Name</FormLabel>
              <FormControl>
                <Input placeholder="name" {...field} />
              </FormControl>
              <FormMessage />
            </FormItem>
          )}
        />
        <FormField
          control={form.control}
          name="create_option"
          render={({ field }) => (
            <FormItem>
              <FormLabel>Create option</FormLabel>
              <Select onValueChange={field.onChange} defaultValue={field.value}>
                <FormControl>
                  <SelectTrigger>
                    <SelectValue placeholder="Select a verified email to display" />
                  </SelectTrigger>
                </FormControl>
                <SelectContent>
                  <SelectItem value="error">error</SelectItem>
                  <SelectItem value="ignore_if_exists">
                    ignore_if_exists
                  </SelectItem>
                </SelectContent>
              </Select>
              <FormMessage />
            </FormItem>
          )}
        />
      </form>
    </Form>
  );
}
