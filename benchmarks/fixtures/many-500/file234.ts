interface Item234 { id: number; name: string; }
type Key234 = string | number;
export function make234(id: number, name: string): Item234 {
  const value: Item234 = {id, name};
  return value as Item234;
}
export const item234: Item234 = make234(234, "item-234");
