interface Item230 { id: number; name: string; }
type Key230 = string | number;
export function make230(id: number, name: string): Item230 {
  const value: Item230 = {id, name};
  return value as Item230;
}
export const item230: Item230 = make230(230, "item-230");
