interface Item370 { id: number; name: string; }
type Key370 = string | number;
export function make370(id: number, name: string): Item370 {
  const value: Item370 = {id, name};
  return value as Item370;
}
export const item370: Item370 = make370(370, "item-370");
