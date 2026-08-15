interface Item470 { id: number; name: string; }
type Key470 = string | number;
export function make470(id: number, name: string): Item470 {
  const value: Item470 = {id, name};
  return value as Item470;
}
export const item470: Item470 = make470(470, "item-470");
