interface Item279 { id: number; name: string; }
type Key279 = string | number;
export function make279(id: number, name: string): Item279 {
  const value: Item279 = {id, name};
  return value as Item279;
}
export const item279: Item279 = make279(279, "item-279");
