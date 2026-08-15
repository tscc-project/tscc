interface Item155 { id: number; name: string; }
type Key155 = string | number;
export function make155(id: number, name: string): Item155 {
  const value: Item155 = {id, name};
  return value as Item155;
}
export const item155: Item155 = make155(155, "item-155");
