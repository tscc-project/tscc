interface Item262 { id: number; name: string; }
type Key262 = string | number;
export function make262(id: number, name: string): Item262 {
  const value: Item262 = {id, name};
  return value as Item262;
}
export const item262: Item262 = make262(262, "item-262");
