interface Props37 { value: number; label: string }
export const View37 = (p: Props37) => (
  <section data-i="37">
    <h2>{p.label}</h2>
    {p.value > 37 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
