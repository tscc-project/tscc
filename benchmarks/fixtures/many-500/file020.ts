interface Item20 { id: number; name: string; }
type Key20 = string | number;
export function make20(id: number, name: string): Item20 {
  const value: Item20 = {id, name};
  return value as Item20;
}
export const item20: Item20 = make20(20, "item-20");
