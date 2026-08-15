interface Item122 { id: number; name: string; }
type Key122 = string | number;
export function make122(id: number, name: string): Item122 {
  const value: Item122 = {id, name};
  return value as Item122;
}
export const item122: Item122 = make122(122, "item-122");
