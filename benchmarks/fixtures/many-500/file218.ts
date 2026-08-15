interface Item218 { id: number; name: string; }
type Key218 = string | number;
export function make218(id: number, name: string): Item218 {
  const value: Item218 = {id, name};
  return value as Item218;
}
export const item218: Item218 = make218(218, "item-218");
