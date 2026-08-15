interface Item326 { id: number; name: string; }
type Key326 = string | number;
export function make326(id: number, name: string): Item326 {
  const value: Item326 = {id, name};
  return value as Item326;
}
export const item326: Item326 = make326(326, "item-326");
