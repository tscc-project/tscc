interface Item140 { id: number; name: string; }
type Key140 = string | number;
export function make140(id: number, name: string): Item140 {
  const value: Item140 = {id, name};
  return value as Item140;
}
export const item140: Item140 = make140(140, "item-140");
