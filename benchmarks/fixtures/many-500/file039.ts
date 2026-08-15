interface Item39 { id: number; name: string; }
type Key39 = string | number;
export function make39(id: number, name: string): Item39 {
  const value: Item39 = {id, name};
  return value as Item39;
}
export const item39: Item39 = make39(39, "item-39");
