interface Item29 { id: number; name: string; }
type Key29 = string | number;
export function make29(id: number, name: string): Item29 {
  const value: Item29 = {id, name};
  return value as Item29;
}
export const item29: Item29 = make29(29, "item-29");
