interface Item177 { id: number; name: string; }
type Key177 = string | number;
export function make177(id: number, name: string): Item177 {
  const value: Item177 = {id, name};
  return value as Item177;
}
export const item177: Item177 = make177(177, "item-177");
