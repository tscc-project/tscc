interface Item444 { id: number; name: string; }
type Key444 = string | number;
export function make444(id: number, name: string): Item444 {
  const value: Item444 = {id, name};
  return value as Item444;
}
export const item444: Item444 = make444(444, "item-444");
