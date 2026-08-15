interface Item191 { id: number; name: string; }
type Key191 = string | number;
export function make191(id: number, name: string): Item191 {
  const value: Item191 = {id, name};
  return value as Item191;
}
export const item191: Item191 = make191(191, "item-191");
